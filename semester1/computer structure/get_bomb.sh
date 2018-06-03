#!/bin/bash

echo "Enter your ID[Teudat_Zehut] number:"
read id

echo "Enter your U2 user:"
read user

echo "Enter your Full Bar-Ilan Email Address:"
read mail

query_string="http://127.0.0.1:15213/?username=$user&usermail=$mail&submit=Submit"

wget_cmd="wget --content-disposition $query_string"

ssh_cmd="ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o LogLevel=quiet -N -L 15213:127.0.0.1:15213 $user@u2.cs.biu.ac.il"

echo "Please enter your u2 password in the NEW TERMINAL THAT JUST OPENED"
gnome-terminal -e "$ssh_cmd"
ssh_pid=$!
read -p "After you have entered your password IN THE OTHER TERMINAL, press [Enter] here"

echo "Trying to download bomb file..."
wget -q -t 3 --content-disposition $query_string

if [ $? -eq 0 ]
then
  echo "File downloaded Successfully:"
  ls -la bomb*
else
  echo "File download failed :("
fi

echo "Closing ssh tunnel"
pkill -f "$ssh_cmd"

echo "Have a nice day =)"
