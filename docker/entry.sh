#!/bin/bash

# build the os-image and then hang so it can be copied out of the container
cd /edOS
make os-image
sleep infinity