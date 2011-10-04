/*
 * Copyright 2011, Red Hat, Inc.
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
 */

#ifndef __SMIS_H__
#define __SMIS_H__

#include <Pegasus/Common/Config.h>
#include <Pegasus/Client/CIMClient.h>
#include <libstoragemgmt/libstoragemgmt_common.h>
#include <libstoragemgmt/libstoragemgmt_types.h>
#include <libstoragemgmt/libstoragemgmt_plug_interface.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

/**
 * Class for Smis block operations.
 */
class LSM_DLL_LOCAL Smis
{
public:

    /**
     * Class constructor.
     * Once this completes we have a connection to the SMI-S agent/proxy.
     * @param   host    a string representing the IP or host of SMI-S agent
     * @param   port    The server port to connect too.
     * @param   smisNameSpace   The SMI-S namespace to use.
     * @param   userName    User name when using authentication.
     * @param   password    Plain text password.
     * @param   timeout Timeout in ms
     */
    Smis(  String host, Uint16 port, String smisNameSpace,
                                String userName, String password, int timeout);

    /**
     * Class destructor which closes the connection to the SMI-S agent/proxy
     */
    ~Smis();

    /**
     * Set the connection timeout
     * @param timeout in ms
     */
    void setTmo(Uint32 timeout);

    /**
     * Retrieve the connection timeout.
     * @return Number of ms for timeout
     */
    Uint32 getTmo();

    /**
     * Creates a logical unit.
     * @param   storagePoolName     Name of storage pool to allocate lun from.
     * @param   name                Name to be given to new lun.
     * @param   size                Size of the new Lun
     * @throws  Exception
     */
    void createLun( String storagePoolName, String name, Uint64 size);

    /**
     * Creates a snapshot of a lun (point in time copy)
     * @param   sourceLun   Name of lun to snapshot.
     * @param   destStoragePool Storage pool to create snapshot from.
     * @param   destName    Name of new snapshot.
     * @throws  Exception
     */
    void createSnapShot(    String sourceLun, String destStoragePool,
                            String destName);

    /**
     * Deletes a logical unit.
     * @param   name    Name of lun to delete.
     * @throws Exception
     */
    void deleteLun( String name );

    /**
     * Resizes an existing Lun.
     * @param   name    Name of lun to resize
     * @param   size    New size
     * @throws Exception
     */
    void resizeLun( String name, Uint64 size);

    /**
     * Returns an array of lsmPoolPtr
     * @return An array of lsmPoolPtr's
     * @throws  Exception on error
     */
    lsmPoolPtr *getStoragePools(Uint32 *count);

    /**
     * Returns an array of Strings which are the names of the logical units
     * @return  An Array<String>
     * @throws  Exception
     */
    Array<String> getLuns();

    /**
     * Returns an array of Strings which are the ID(s) of the initiators
     * @return An Array<String> of initiator IDs
     * @throws Exception
     */
    Array<String> getInitiators();

    /**
     * Grants read/write access for a lun to the specified initiator.
     * @param   initiatorID     The initiator ID
     * @param   lunName         The lun name
     * @throws Exception
     */
    void mapLun(String initiatorID, String lunName);

private:
    enum ElementType { UNKNOWN = 0, RESERVED = 1, STORAGE_VOLUME = 2,
                       STORAGE_EXTENT = 3, STORAGE_POOL = 4, LOGICAL_DISK = 5
                     };

    enum DeviceAccess { READ_WRITE = 2, READ_ONLY = 3,
                        NO_ACCESS = 4
                      };

    enum SyncType { MIRROR = 6, SNAPSHOT = 7, CLONE = 8 };

    enum Mode { SYNC = 2, ASYNC = 3 };

    enum MethodInvoke { INVOKE_OK = 0, INVOKE_ASYNC = 4096 };

    enum OperationalStatus { OK = 2, ERROR = 6, STOPPED = 10, COMPLETE = 17 };

    CIMClient   c;
    CIMNamespaceName    ns;

    Array<CIMInstance>  storagePools();
    Array<String> instancePropertyNames( String className, String prop );

    String getClassValue(CIMInstance &instance, String propName);
    CIMInstance getClassInstance(String className);
    CIMInstance getClassInstance(String className, String propertyName,
                                 String propertyValue );

    void evalInvoke(Array<CIMParamValue> &out, CIMValue value,
                    String jobKey = "Job");

    void processJob(CIMValue &job);
};

#endif
