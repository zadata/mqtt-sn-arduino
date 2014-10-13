Run the MQTT Broker, MQTT-SN Gateway and one ore more Arduino/XBee sensor nodes.

Use the following sketch on Arduino node: mqttsn_client.ino



To subscribe to topics:

```
mosquitto_sub -h localhost -t 'sensors/#' -v
```

The output should be:

```
sensors/cl-0013a200408b2ed/light_level 49.0000
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/light_level 48.0000
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/light_level 49.0000
sensors/cl-0013a200408b2ed/led1 on
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/light_level 48.0000
sensors/cl-0013a200408b2ed/led1 off
sensors/cl-0013a200408b2ed/temperature 42.7735
sensors/cl-0013a200408b2ed/light_level 48.0000
```

To enable LED (replace `cl-XXX` client ID with the real one):

```
mosquitto_pub -h localhost -t 'sensors/cl-0013a200408b2ed/led1' -m 'on'
```

To disable LED (replace `cl-XXX` client ID with the real one):

```
mosquitto_pub -h localhost -t 'sensors/cl-0013a200408b2ed/led1' -m 'off'
```
