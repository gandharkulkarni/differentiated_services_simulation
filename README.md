# A Base Class to Simulate Differentiated Services
Network programming project : A Base Class to Simulate Differentiated Services
## Developed by
- Gandhar Kulkarni
- Vinay Bojja
- Soham Kalse

<!-- (2) complete instructions on how one can build -->
## How to run the code

Installing the dependencies
```
apt install g++ python3 cmake ninja-build git
```

Setting up project
```
mkdir Project2
cd Project2
git clone https://gitlab.com/nsnam/ns-3-allinone.git
cd ns-3-allinone
```
Building the project
```
./download.py -n ns-3.38
./build.py
```
Testing the if project is setup correctly
```
./test.py
```

```
cd ns-3.38
```
- Copy the folders and files from /scratch (Github repo) and paste in the ns-3.38/scratch folder.
- Copy the files from src/traffic-control/model(Github repo) folder and paste with the files present in ns-3.38/src/traffic-control/model.
- Copy the file src/traffic-control/CMakeLists.txt(Github repo) and paste or make necessary changes by adding file names in source and header section to ns-3.38/src/traffic-control/CmakeLists.txt.

```
cd ..
./build.py
cd ns-3.38
./ns3 configure
```
To execute SPQ
```
./ns3 run "starter --filename=SPQ"
```
To execute DRR
```
./ns3 run "starter --filename=DRR"
```

<!-- (3) the incomplete required features of your program.2 -->
