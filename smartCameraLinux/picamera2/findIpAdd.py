import netifaces

class getInetIPAdd():
    def __init__(self):
        self.iface = "wlan0"
    def find_gateway(self):
      Interfaces= netifaces.interfaces()
      for inter in Interfaces:
           if inter == self.iface:
                temp_list = []
                Addresses = netifaces.ifaddresses(inter)
                gws = netifaces.gateways()
                temp_list = list (gws['default'][netifaces.AF_INET])
                count =0
                for item in temp_list:
                      count +=1
                      if count ==1:
                           print (item)
                      else:
                           pass
                return count

