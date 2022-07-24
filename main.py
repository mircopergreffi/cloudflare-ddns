
import os, time, yaml, requests, re, CloudFlare

def get_config():
    # Read config file

    if (os.path.isfile('config.yml')):
        with open('config.yaml', 'r') as ymlfile:
            try:
                cfg = yaml.safe_load(ymlfile)
            except:
                print('Error: Could not load config file')
                exit(1)
    else:
        cfg = {
            'cloudflare':
            {
                'email': os.getenv('CLOUDFLARE_EMAIL'),
                'token': os.getenv('CLOUDFLARE_TOKEN'),
            },
            'update_interval': os.getenv('UPDATE_INTERVAL', '300'),
            'update_ip': [
                {
                    'zone_id': os.getenv('ZONE_ID_1'),
                    'id': os.getenv('DNS_RECORD_ID_1'),
                },
                {
                    'zone_id': os.getenv('ZONE_ID_2'),
                    'id': os.getenv('DNS_RECORD_ID_2'),
                }
            ]
        }
        
    return cfg

def get_ipaddr():
    # Get current IP address
    try:
        r = requests.get('https://api.ipify.org')
        return r.text
    except:
        print('Error: Could not get IP address')
        return None
    # content = requests.get('http://192.168.1.1/network-expert-internet.lp?ip=&phoneType=undefined').content.decode('utf-8')
    # content = content.replace(' ', '')
    # content = content.replace('\n', '')
    # content = content.replace('\r', '')
    # return re.search('<dt>IndirizzoIPv4</dt><dd>([0-9.]+)</dd>', content).group(1)

def update_dns(cfg, ipaddr):
    cf = CloudFlare.CloudFlare(email=cfg['cloudflare']['email'], token=cfg['cloudflare']['token'])
    for entry in cfg['update_ip']:
        dnsrecord = cf.zones.dns_records.get(entry['zone_id'], entry['id'])
        data = {'type':dnsrecord['type'], 'name':dnsrecord['name'], 'content':ipaddr, 'ttl':dnsrecord['ttl'], 'proxied':dnsrecord['proxied']}
        cf.zones.dns_records.put(entry['zone_id'], entry['id'], data=data)

if __name__ == '__main__':
    cfg = get_config()
    current_ip = ''
    while True:
        new_ip = get_ipaddr()
        if new_ip != None:
            if new_ip != current_ip:
                print('New IP: ' + new_ip)
                update_dns(cfg, new_ip)
                current_ip = new_ip
            else:
                print('IP unchanged')
        time.sleep(int(cfg['update_interval']))