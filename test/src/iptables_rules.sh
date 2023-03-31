#!/bin/bash

echo "Adding rules to block serveral protocols"
sudo iptables -A INPUT -i tun0 -p icmp -j DROP
sudo iptables -A INPUT -i tun0 -p ipv6-icmp -j DROP
sudo iptables -A INPUT -i tun0 -p tcp -j DROP
sudo iptables -A INPUT -i tun0 -p udp -j DROP
sudo iptables -A INPUT -i tun0 -p udp --dport 1900 -j DROP

sudo iptables -A OUTPUT -o tun0 -p icmp -j DROP
sudo iptables -A OUTPUT -o tun0 -p ipv6-icmp -j DROP
sudo iptables -A OUTPUT -o tun0 -p tcp -j DROP
sudo iptables -A OUTPUT -o tun0 -p udp -j DROP
sudo iptables -A OUTPUT -o tun0 -p udp --dport 1900 -j DROP

sudo iptables -A FORWARD -i tun0 -p icmp -j DROP
sudo iptables -A FORWARD -i tun0 -p ipv6-icmp  -j DROP
sudo iptables -A FORWARD -i tun0 -p tcp -j DROP
sudo iptables -A FORWARD -i tun0 -p udp -j DROP
sudo iptables -A FORWARD -i tun0 -p udp --dport 1900 -j DROP

sudo iptables -A FORWARD -o tun0 -p icmp -j DROP
sudo iptables -A FORWARD -o tun0 -p ipv6-icmp  -j DROP
sudo iptables -A FORWARD -o tun0 -p tcp -j DROP
sudo iptables -A FORWARD -o tun0 -p udp -j DROP
sudo iptables -A FORWARD -o tun0 -p udp --dport 1900 -j DROP

sudo iptables -L -vnx
