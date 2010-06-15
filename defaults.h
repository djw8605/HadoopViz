#ifndef CONFIG_H_
#define CONFIG_H_

/* Field of view, used in viewport setting */
#define FOV 45.0

// Area of the surface
#define AREA_SIZE 60

// Space between hosts
#define SPACE_BETWEEN 4

// Height the drop and floats
#define DROP_CREATION 40.0
#define DROP_HEIGHT DROP_CREATION

// I'm lazy
#define BYTES_PER_MEGABYTE 1048576.0

// Number of seconds to average the transfers over
#define EVENTTIMEOUT 60



/* Default Values */
#define DEFAULT_SERVER "viz.unl.edu"
#define DEFAULT_PORT 5679
#define DEFAULT_DATADIR "./"


#endif /*CONFIG_H_*/
