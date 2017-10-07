#!/bin/sh

echo "preparing the latest version of qhttp ...\n"

echo "--> qhttp"
if [ -e qhttp ]; then
    cd qhttp
    git pull origin master
    cd ..
else
    git clone https://github.com/azadkuh/qhttp.git
fi

cd qhttp
./update-dependencies.sh

# cd ..
echo "done."