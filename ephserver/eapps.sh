#!/bin/bash

echo "Invoking Famulous ..."
node famulous.sh | tee famulous.log &

echo "Invoking conjurer ..."
node conjurer.sh | tee conjurer.log & 
