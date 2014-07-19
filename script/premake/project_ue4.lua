--[[Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. ]]

local projectInfo = ...

external "plugin_ue4"
  location "ue4/Intermediate/ProjectFiles"
  kind "StaticLib"
  removeconfigurations { "Debug" }
  
  configmap { -- these configs should be mapped, one way or another, but right now there's no mapping that makes sense
    --[{"Debug", "x32"}] = {"Shipping_RocketGame", "x32"},
    [{"Release", "x32"}] = {"Shipping_RocketGame", "x32"},
    --[{"Debug", "x64"}] = {"Development_Editor", "x64"},
    [{"Release", "x64"}] = {"Development_Editor", "x64"},
  }

  links {"frames", "frames_null", "frames_dx11", "frames_opengl"}