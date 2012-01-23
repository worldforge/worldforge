/**
 * varconf_defs.h - project-wide definitions for the varconf library
 *
 * Copyright 2012 Stephen M. Webb <stephen.webb@bregmasoft.ca>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef VARCONF_VARCONF_DEFS_H
#define VARCONF_VARCONF_DEFS_H

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_VARCONF_DSO
    #define VARCONF_API __declspec(dllexport)
  #else
    #define VARCONF_API __declspec(dllimport)
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define VARCONF_API __attribute__ ((visibility ("default")))
  #else
    #define VARCONF_API
  #endif
#endif

#endif /* VARCONF_VARCONF_DEFS_H */
