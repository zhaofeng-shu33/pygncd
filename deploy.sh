if [ "$TRAVIS_OS_NAME" == "osx" ]; then
   python3 -m twine upload dist/* -u zhaofeng-shu33 && echo success || echo "upload failed"
else
   python -m twine upload dist/wheelhouse/*.whl -u zhaofeng-shu33 && echo success || echo "upload failed"
fi