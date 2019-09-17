# build and test script on unix
cd build
cmake ..
make
cp ../test/karate.gml ./
test/test
cd ..
python setup.py build_ext --inplace
pip install -e .
python test/test.py
python test/test_GN.py
