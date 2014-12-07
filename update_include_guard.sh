#!/bin/bash
guard_name_prefix=_solar_wind_lisp_
for fn in "$@"
do
    old_guard=$(grep '#ifndef _.*H_$' "$fn" | gawk '{print $2}')
    base=$(basename "$fn" | sed -e "s/\.h/_h_/")
    # new include guard, in upper case
    new_guard=$(echo ${guard_name_prefix}${base} | tr 'a-z' 'A-Z')
    if [ x"$old_guard" != x"" ]
    then
        sed -i -e "s/$old_guard/$new_guard/" $fn
    fi
done
