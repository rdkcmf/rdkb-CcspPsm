/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**********************************************************************

    module:	psm_sysro_states.c

        For Persistent Storage Manager Implementation (PSM),
        Common Component Software Platform (CCSP)

    ---------------------------------------------------------------

                    Copyright (c) 2011, Cisco Systems, Inc.

                            CISCO CONFIDENTIAL
              Unauthorized distribution or copying is prohibited
                            All rights reserved

 No part of this computer software may be reprinted, reproduced or utilized
 in any form or by any electronic, mechanical, or other means, now known or
 hereafter invented, including photocopying and recording, or using any
 information storage and retrieval system, without permission in writing
 from Cisco Systems, Inc.

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the Psm Sys Registry Object.

        *   PsmSysroGetPsmSseIf
        *   PsmSysroSetPsmSseIf
        *   PsmSysroGetPsmFileLoader
        *   PsmSysroGetSysInfoRepository
        *   PsmSysroGetProperty
        *   PsmSysroSetProperty
        *   PsmSysroResetProperty
        *   PsmSysroReset

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Jian Wu

    ---------------------------------------------------------------

    revision:

        06/17/11    initial revision.

**********************************************************************/


#include "psm_sysro_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        PsmSysroGetPsmSseIf
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
PsmSysroGetPsmSseIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    return  pMyObject->hPsmSseIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        PsmSysroSetPsmSseIf
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hInterface
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hInterface
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
PsmSysroSetPsmSseIf
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    pMyObject->hPsmSseIf = hInterface;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        PsmSysroGetPsmFileLoader
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
PsmSysroGetPsmFileLoader
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    return  pMyObject->hPsmFileLoader;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        PsmSysroGetSysInfoRepository
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
PsmSysroGetSysInfoRepository
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    return  pMyObject->hSysInfoRepository;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        PsmSysroGetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to retrieve object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be filled.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
PsmSysroGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    *(PPSM_SYS_REGISTRY_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        PsmSysroSetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to configure object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be copied.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
PsmSysroSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    *pProperty = *(PPSM_SYS_REGISTRY_PROPERTY)hProperty;

    if ( AnscSizeOfString(pProperty->BakFileName) == 0 )
    {
        AnscCopyString(pProperty->BakFileName, PSM_DEF_BAK_FILE_NAME);
    }

    if ( AnscSizeOfString(pProperty->TmpFileName) == 0 )
    {
        AnscCopyString(pProperty->TmpFileName, PSM_DEF_TMP_FILE_NAME);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        PsmSysroResetProperty
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
PsmSysroResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    AnscZeroMemory(pProperty, sizeof(PSM_SYS_REGISTRY_PROPERTY));

    AnscCopyString(pProperty->SysFilePath, PSM_DEF_SYS_FILE_PATH);
    AnscCopyString(pProperty->DefFileName, PSM_DEF_DEF_FILE_NAME);
    AnscCopyString(pProperty->CurFileName, PSM_DEF_CUR_FILE_NAME);
    AnscCopyString(pProperty->BakFileName, PSM_DEF_BAK_FILE_NAME);
    AnscCopyString(pProperty->TmpFileName, PSM_DEF_TMP_FILE_NAME);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        PsmSysroReset
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object states.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
PsmSysroReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PPSM_SYS_REGISTRY_OBJECT       pMyObject    = (PPSM_SYS_REGISTRY_OBJECT  )hThisObject;
    PPSM_SYS_REGISTRY_PROPERTY     pProperty    = (PPSM_SYS_REGISTRY_PROPERTY)&pMyObject->Property;

    return  ANSC_STATUS_SUCCESS;
}
