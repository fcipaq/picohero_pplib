/*
 * pplib - a library for the Pico Hero handheld
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

#define blit_tile_map(kx, ky, w, h, px, py, zoom_x, zoom_y, map_data, tile_set, fb) \
        blit_tile_map_rot(kx, ky, w, h, px, py, 0, 0, 0, zoom_x, zoom_y, map_data, tile_set, fb)
	
void blit_tile_map_rot(coord_t kx,  // start in fb window x
					   coord_t ky,  // start in fb window y
					   coord_t w,   // window width
					   coord_t h,   // window height
					   coord_t px,  // translation within window
					   coord_t py,  
					   coord_t pivot_x,  // pivot point (in screen/buffer coords, 0/0 is upper left corner)
					   coord_t pivot_y,  
					   float rot,
					   float zoom_x,
					   float zoom_y,
					   tile_map_t map_data,
					   tile_data_t tile_set,
					   gbuffer_t buf);

void blit_tile_map_mode7(coord_t kx,  // start in fb window x
					     coord_t ky,  // start in fb window y
					     coord_t w,   // window width
					     coord_t h,   // window height
					     float px,  // translation within window
					     float py,  // translation within window
					     float pz,  // translation within window
					     float pr,  // rotation within window
					     tile_map_t map_data,
					     tile_data_t tile_set,
					     gbuffer_t fb);
					  
#endif //TILEMAP_H
