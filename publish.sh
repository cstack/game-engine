make clean
make all

cp index.html chrome-app/window.html
cp game.pexe chrome-app/
cp game.nmf chrome-app/

zip -r chrome-app.zip chrome-app/