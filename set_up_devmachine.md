Instructions for creating a new Debian machine for developing in it:

Download and install Debian/arm64, e.g. from https://cdimage.debian.org/debian-cd/current/arm64/iso-cd/

currently user/pass dubon/dubon, root pass dubon and using XFCE as desktop environmrnt

use always the root/dubon user

`apt install git`

take token from tom

`git clone --recursive https://tom-programming:<TOKEN>@github.com/tom-programming/InfiniTime `

and configure git:
```
git config --global user.email "tom.programs@gmail.com"
git config --global user.name "Dubon"
```

install software from Dockerfile:

```
apt-get update -qq
apt-get install -y bash build-essential cmake git make python3 python3-pip python-is-python3 tar unzip wget curl
apt-get install -y libffi-dev libssl-dev python3-dev git apt-utils pkg-config libpixman-1-dev libcairo2-dev libpango-1.0-0 ibpango1.0-dev libpangocairo-1.0-0
curl -sL https://deb.nodesource.com/setup_18.x | bash -
apt-get install -y nodejs
rm -rf /var/cache/apt/* /var/lib/apt/lists/*

pip3 install adafruit-nrfutil
pip3 install -Iv cryptography==3.3
pip3 install cbor
npm i lv_font_conv@1.5.2 -g

npm i ts-node@10.9.1 -g
npm i @swc/core -g
npm i lv_img_conv@0.3.0 -g

cp InfiniTime/docker/build.sh /opt/
bash -c "source /opt/build.sh; GetGcc;"
bash -c "source /opt/build.sh; GetNrfSdk;"
bash -c "source /opt/build.sh; GetMcuBoot;"

ln -s /root/InfiniTime /sources
```

install vscode
```
wget "https://code.visualstudio.com/sha/download?build=stable&os=linux-deb-arm64" -O code.deb
apt install ./code.deb
```

to open VsCode
```
code --no-sandbox --user-data-dir /home/dubon InfiniTime/
```

change to our branch

```
cd InfiniTime
git checkout tom/heartrate
```

test build

```
/opt/build.sh
```

### Final status
Files will be in `/root/InfiniTime`, VSCode can be opened by `code --no-sandbox --user-data-dir /home/dubon /root/InfiniTime/`
