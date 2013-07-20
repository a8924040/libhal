Usage: Usage: dhcpclient [-d(debug mode)] [-i interface name] [-t dhcp timeout] [-T AutoIP sleep time] [-g Gateway]
Function: If dhcp server is available, contact it to get an IP address. 
Otherwise use AutoIP policy and run as a daemon to periodically check whether the DHCP server is available.
Default paremeter values: interface name = "ixp0"; dhcp timeout = 10s; AutoIP sleep time = 300s