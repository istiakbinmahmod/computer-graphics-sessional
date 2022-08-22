if [ $# == 0 ];then
    echo "No arguments! Please input the file name!(e.g. 1/2)"
    exit 1
fi

g++ $1.cpp -o task$1 -lglut -lGLU -lGL
./task$1
