/*
Copyright (C) 2011 Salvy6735

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "BuildOptions.h"
#include "Base/Types.h"
#include "SysPSP/Utility/ModulePSP.h"

#include <stdio.h>
#include <iostream>


#include <pspsdk.h>

namespace CModule
{
	void Unload( int id )
	{
		int ret {0}, status {0};
		sceKernelStopModule(id, 0, nullptr, &status, nullptr);	// Stop module first before unloading it

		ret = sceKernelUnloadModule(id);

		if(ret < 0)
		{
			printf("Couldn't unload module! : 0x%08X\n",ret);
		}
	}

	int Load( const std::filesystem::path path )
	{
		int ret = pspSdkLoadStartModule(path.c_str()
		, PSP_MEMORY_PARTITION_KERNEL);

		if( ret < 0 )
		{	
			std::cout << "Failed to load" << path <<": " << ret;
			// printf( "Failed to load %s: %d\n",path, ret );
			return ret; //-1
		}

		printf( "Successfully loaded %s: %08X\n", path, ret );

		return ret;
	}
}
