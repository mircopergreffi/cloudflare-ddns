#!/bin/sh
current_ip=0.0.0.0
while :; do
  new_ip=$(curl -s https://cloudflare.com/cdn-cgi/trace | grep ip | sed "s/ip=//g")
  
  if [[ "$current_ip" == "$new_ip" ]]; then
    echo "IP unchanged."
  else
    echo "New IP: $new_ip"
    current_ip=$new_ip
    
    # Get DNS records
    dns_records=$(curl -s \
      -X GET "https://api.cloudflare.com/client/v4/zones/${ZONEID}/dns_records" \
      -H "Authorization: Bearer ${TOKEN}" \
      -H "Content-Type:application/json")
    # Check if the response is successful
    if [[ $(echo $dns_records | jq '.success') != "true" ]]; then
      echo "Could not retrieve DNS records"
      echo $(echo $dns_records | jq '.errors')
      exit 1
    fi
    
    len=$(echo $dns_records | jq ".result | length")
    echo "Found $len DNS records."

    i=0
    while [[ "$i" -lt $len ]]; do
      record_type=$(echo $dns_records | jq ".result[$i].type")
      record_type=${record_type//\"/}
      record_id=$(echo $dns_records | jq ".result[$i].id")
      record_name=$(echo $dns_records | jq ".result[$i].name")
      record_id=${record_id//\"/}
      record_name=${record_name//\"/}
      i=$((i+1))
      if [[ "$record_type" == "CNAME" ]]; then
        continue
      fi
      echo "Deleting record: id=$record_id; name=$record_name"
      curl -s \
        -X DELETE "https://api.cloudflare.com/client/v4/zones/${ZONEID}/dns_records/${record_id}" \
        -H "Authorization: Bearer ${TOKEN}" \
        -H "Content-Type:application/json"
    done

    # Proxied DNS records
    if [ -n "${BIND_TEMPLATE}" ]; then
      bind_data=$BIND_TEMPLATE
      bind_data=${bind_data//\%ip\%/${current_ip}}
      bind_data=${bind_data//\%domain\%/${DOMAIN}}
      echo "${bind_data//;/$'\n'}" > bind_data.txt
      
      echo Importing Proxied records
      curl -s \
        -X POST "https://api.cloudflare.com/client/v4/zones/${ZONEID}/dns_records/import" \
        -H "Authorization: Bearer ${TOKEN}" \
        --form 'file=@bind_data.txt' \
        --form 'proxied=true'
    fi

    # Non Proxied DNS records
    if [ -n "${BIND_TEMPLATE_NOPROXY}" ]; then
      bind_data_noproxy=$BIND_TEMPLATE_NOPROXY
      bind_data_noproxy=${bind_data_noproxy//\%ip\%/${current_ip}}
      bind_data_noproxy=${bind_data_noproxy//\%domain\%/${DOMAIN}}
      echo "${bind_data_noproxy//;/$'\n'}" > bind_data_noproxy.txt
    
      echo Importing Non Proxied records
      curl -s \
        -X POST "https://api.cloudflare.com/client/v4/zones/${ZONEID}/dns_records/import" \
        -H "Authorization: Bearer ${TOKEN}" \
        --form 'file=@bind_data_noproxy.txt' \
        --form 'proxied=false'
    fi

  fi

  
  if [[ $UPDATE_INTERVAL -ge 0 ]]; then
    sleep $UPDATE_INTERVAL
  else
    break
  fi

done