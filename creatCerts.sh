#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "$0 <atleast 1 IP>"
fi

# Generate root certificate
ndnsec-list | grep autondn
alreadyExist=`echo $?`

if [ "$alreadyExist" -ne 0 ]; then
  echo "Creating and installing root certificate"
  sudo ndnsec-keygen $root > root.keys
  sudo ndnsec-cert-gen -N $root -p $root root.keys > root.cert
  sudo ndnsec-cert-install -f root.cert
else
  echo "Root certificate already exists"
fi

for ip in "$@"
do
  ssh_host="ssh -t -i $HOME/.ssh/id_rsa.pub pi@$ip"

  root="/autondn"

  workDir="/home/mini-ndn/pi/pi/bin"

  scp -i $HOME/.ssh/id_rsa.pub root.cert pi@$ip:$workDir/autondn-root.cert  > /dev/null

  # Generate manufacturer keys on the vehicle

  $ssh_host "cat $workDir/auto-ndn.conf" > tmp1

  man=`cat tmp1 | grep "make /" | awk '{print $2}' | sed 's:/::'g`
  car=`cat tmp1 | grep "id /" | awk '{print $2}' | sed 's:/::'g`

  man_pref="/$man"
  car_pref="/$man/$car"

  #echo "$man_pref"
  echo "For vehicle: $car_pref $ip"

  man_cert=`cat tmp1 | grep "cert-to-publish" | grep $man | awk '{print $2}' | sed 's/\"\|\\r//g'`

  $ssh_host "$workDir/ndnsec-keygen $man_pref > $workDir/man.keys"
  scp pi@$ip:$workDir/man.keys man.keys > /dev/null
  sudo ndnsec-cert-gen -N $man_pref -p $man_pref -s $root man.keys > $man_cert
  scp "$man_cert" pi@$ip:$workDir/$man_cert  > /dev/null
  rm man.keys
  rm $man_cert

  $ssh_host "$workDir/ndnsec-cert-install -f $workDir/$man_cert"

  car_cert=`cat tmp1 | grep "cert-to-publish" | grep $car | awk '{print $2}' | sed 's/\"\|\\r//g'`

  #echo $car_cert

$ssh_host << EOF
  $workDir/ndnsec-keygen $car_pref > $workDir/car.keys
  $workDir/ndnsec-cert-gen -N $car_pref -p $car_pref -s $man_pref $workDir/car.keys > $workDir/$car_cert
  $workDir/ndnsec-cert-install -f $workDir/$car_cert
EOF
done
rm tmp1
