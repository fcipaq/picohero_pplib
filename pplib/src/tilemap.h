/*
 * pplib - a library for the PicoPal handheld
 *
 * Copyright (C) 2023 Daniel Kammer (daniel.kammer@web.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#ifndef TILEMAP_H
#define TILEMAP_H

#define tile_blit(kx, ky, w, h, px, py, zoom_x, zoom_y, map_data, tile_set, alpha, fb) \
        tile_blit_rot(kx, ky, w, h, px, py, 0, 0, 0., zoom_x, zoom_y, map_data, tile_set, alpha, fb)
	
void tile_blit_rot(coord_t kx,            // start in fb window x
                   coord_t ky,            // start in fb window y
                   coord_t w,             // window width
                   coord_t h,             // window height
                   coord_t px,            // translation within window
                   coord_t py,  
                   coord_t pivot_x,       // pivot point (in screen/buffer coords, 0/0 is upper left corner)
                   coord_t pivot_y,  
                   float rot,             // angle
                   float zoom_x,          // zoom in horizontal direction
                   float zoom_y,          // zoom in vertical direction
                   tile_map_t map_data,   // map data
                   tile_data_t tile_set,  // tile data
                   int32_t alpha,         // transparency
                   gbuffer_t buf);        // pointer to destination buffer

void tile_blit_mode7(coord_t kx,           // start in fb window x
                     coord_t ky,           // start in fb window y
                     coord_t w,            // window width
                     coord_t h,            // window height
                     float px,             // translation within window
                     float py,             // translation within window
                     float pz,             // translation within window
                     float pr,             // rotation within window
                     tile_map_t map_data,  // map data
                     tile_data_t tile_set, // tile data
                     int32_t alpha,        // transparency
                     gbuffer_t buf);       // pointer to destination buffer
					  
#endif //TILEMAP_H
