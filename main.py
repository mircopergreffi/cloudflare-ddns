
import yaml
import requests
import re
import CloudFlare

if __name__ == '__main__':

    # Read config file
    with open('config.yaml', 'r') as ymlfile:
        try:
            cfg = yaml.safe_load(ymlfile)
        except:
            print('Error: Could not load config file')
            exit(1)

    # Get IP address
    content = requests.get('http://192.168.1.1/network-expert-internet.lp?ip=&phoneType=undefined').content.decode('utf-8')
    content = content.replace(' ', '')
    content = content.replace('\n', '')
    content = content.replace('\r', '')
    ipaddr = re.search('<dt>IndirizzoIPv4</dt><dd>([0-9.]+)</dd>', content).group(1)
    
    cf = CloudFlare.CloudFlare(email=cfg['cloudflare']['email'], token=cfg['cloudflare']['token'])
    for entry in cfg['update_ip']:
        dnsrecord = cf.zones.dns_records.get(entry['zone_id'], entry['id'])
        data = {'type':dnsrecord['type'], 'name':dnsrecord['name'], 'content':ipaddr, 'ttl':dnsrecord['ttl'], 'proxied':dnsrecord['proxied']}
        cf.zones.dns_records.put(entry['zone_id'], entry['id'], data=data)