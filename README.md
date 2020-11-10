# NetCompDetection
Developer: José Corella


# Compression Detection
This project implements an end-to-end compression detection of network traffic intermediaries based on the research done by Vahab Pournaghshband, Alexander Afanasyev, and Peter Reiher.
I have decided to limit only TCP/IP Compression.

# Endpoints
I used two programs the [client](client.c) and the [server](server.c). The client is responsible for sending the configuration file over the wire as well as sending UDP packets of low and high entropy. In order to expose compression, two waves of low entropy and high entropy packets are sent over the wire using UDP. The reason for the high and low entropy is because low entropy packets are subject to compression over their counterparts.

Inspired by the research of Dr.  Vahab Pournaghshband, in order to detect if compression was detected, we take the time it took for the server to receive the low and high entropy packets. Taking the difference in time of these two waves gives us an idea of how long it took for high entropy packets and low entropy packets. I then compared this delta with a threshold value of 100ms. If the result was under the threshold it meant that there was no compression detected, and if the result was over the threshold the program deduced that there was compression and this was reported back to the client.

## Usage for Client
```
make compdetect_client
./compdetect_client config.json
```

## Usage for server
```
make compdetect_server
./compdetect_server
```

## Further Reading
https://lasr.cs.ucla.edu/vahab/resources/compression_detection.pdf

Project Guide found [here](CS486_Fall_2020_Project.pdf)
