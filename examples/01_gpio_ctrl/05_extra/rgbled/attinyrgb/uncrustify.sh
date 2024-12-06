find . \( -name "*.ino" -o -name "*.c" -o -name "*.h" \) -exec ~/bin/uncrustify -c ./uncrustify.cfg --replace --no-backup {} +

