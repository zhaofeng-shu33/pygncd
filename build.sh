# build and test script on unix
mkdir build
cd build
cmake ..
make
cp ../test/karate.gml ./
test/test
cd ..
python3 setup.py bdist_wheel
python3 -m pip install -e .
python3 test/test.py
python3 test/test_GN.py
