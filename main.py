
import os, time, yaml, requests, re, CloudFlare

def get_config():
    # Read config file

    path = 'config.yaml'
    if (not os.path.isfile(path)):
        path = os.path.join(os.getenv("DATA_LOCATION", "/var/lib/data"), path)
        if (not os.path.isfile(path)):
            print('Error: Could not find config.yaml file')
            exit(1) 

    with open(path, 'r') as ymlfile:
        try:
            cfg = yaml.safe_load(ymlfile)
        except:
            print('Error: Could not load config.yaml file')
            exit(1)
        
    return cfg

def get_ipaddr():
    # Get current IP address
    try:
        r = requests.get('https://api.ipify.org')
        return r.text
    except:
        print('Error: Could not get IP address')
    return None

def update_dns(cfg, ipaddr):
    cf = CloudFlare.CloudFlare(email=cfg['cloudflare']['email'], token=cfg['cloudflare']['token'])
    for entry in cfg['update_ip']:
        dnsrecord = cf.zones.dns_records.get(entry['zone_id'], entry['id'])
        data = {'type':dnsrecord['type'], 'name':dnsrecord['name'], 'content':ipaddr, 'ttl':dnsrecord['ttl'], 'proxied':dnsrecord['proxied']}
        cf.zones.dns_records.put(entry['zone_id'], entry['id'], data=data)

def update_ip(cfg):
    try:
        new_ip = get_ipaddr()
        if new_ip != current_ip:
            print('New IP: ' + new_ip)
            update_dns(cfg, new_ip)
            current_ip = new_ip
        else:
            print('IP unchanged')
        return True
    except:
        print('Error: Could not update IP address')
    return False

error = False

if __name__ == '__main__':
    print('Starting...')
    cfg = get_config()
    print(cfg)
    current_ip = ''
    while True:
        if update_ip(cfg):
            time.sleep(int(cfg['update_interval']))
        else:
            time.sleep(int(cfg['update_interval_error']))