sudo mount -t ext4 /dev/sdb2 /media/data_disk
# if [ "$# -ne 1" ]; then 
#    echo "Illegal number of arguments!"
#    echo "Usage: ./mount_disk.sh [device name relative to /dev/]." 
#    echo "-- This is the point to which we mount the external USB drive."
#    exit 1  
# fi 
# 
# sudo mount -t ext4 /dev/$1 /media/data_disk
