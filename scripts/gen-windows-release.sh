#!/bin/bash

cd ../src
make clean
make

exe_path="./sgnotes.exe"
bin_path="sgnotes_release/bin"
share_path="sgnotes_release/share"
icons_path="$share_path/icons"

mkdir -pv "$bin_path"
mkdir -pv "$share_path/glib-2.0"

dependencies=$(ldd "$exe_path" | grep mingw64 | awk '{print $3}' | grep -E "\.dll$")

for dll in $dependencies
do
	cp -v "$dll" "$bin_path"
	echo "Copied $dll to $bin_path"
done

cp -rv /mingw64/share/gtksourceview-4/language-specs  "sgnotes_release/share/gtksourceview-4/language-specs"

cp -rv share "$share_path"
cp -rv /mingw64/share/icons/Adwaita "$icons_path/"
cp -rv /mingw64/share/icons/hicolor "$icons_path/"
cp -rv /mingw64/share/glib-2.0/schemas/* sgnotes_release/share/glib-2.0/schemas/

gtk-update-icon-cache -f "$icons_path/Adwaita"
gtk-update-icon-cache -f "$icons_path/hicolor"
rm sgnotes_release/share/glib-2.0/schemas/gschema*
glib-compile-schemas sgnotes_release/share/glib-2.0/schemas

cp sgnotes.exe "$bin_path"
cp COPYING sgnotes_release
