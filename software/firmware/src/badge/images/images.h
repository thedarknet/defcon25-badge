#ifndef DCDARKNET_IMAGES_H
#define DCDARKNET_IMAGES_H

struct DCImage {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  const char	*pixel_data;
};

const DCImage &getCyberez();
const DCImage &getDC25();

#endif
