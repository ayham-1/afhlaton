./scripts/clean.sh
cmake -B.build -DCMAKE_BUILD_TYPE=Debug -GNinja
ninja -C .build -j 8
cd .build/
./afhlaton
cd ..
