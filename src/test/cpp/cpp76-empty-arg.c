#define CONFIGFS_ATTR_PERM(_pfx, _name)  struct configfs_attribute _pfx##_name

#define CONFIGFS_ATTR(_pfx, _name) CONFIGFS_ATTR_PERM(_pfx , _name)

CONFIGFS_ATTR(, survivability_mode);

#define str(x, y) #x
#define str2(x) str( x , y)

str2(a)
