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


#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_)
#include <execinfo.h>
#endif
#endif

#ifdef _ANSC_LINUX
#include <sys/types.h>
#include <sys/ipc.h>
#ifdef _BUILD_ANDROID
#include <linux/msg.h>
#else
#include <sys/msg.h>
#endif
#endif

#include "ssp_global.h"


BOOL                                bEngaged          = FALSE;
PPSM_SYS_REGISTRY_OBJECT            pPsmSysRegistry   = (PPSM_SYS_REGISTRY_OBJECT)NULL;
void                               *bus_handle        = NULL;
char                                g_Subsystem[32]   = {0};
BOOL                                g_bLogEnable      = FALSE;
extern char*                        pComponentName;
#ifdef USE_PLATFORM_SPECIFIC_HAL
PSM_CFM_INTERFACE                   cfm_ifo;
#endif



static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _NO_EXECINFO_H_)
	void* tracePtrs[100];
	char** funcNames = NULL;
	int i, count = 0;

        int fd;
        const char* path = "/nvram/psmssp_backtrace";
        fd = open(path, O_RDWR | O_CREAT, 0777);
        if (fd < 0)
        {
            fprintf(stderr, "failed to open backtrace file: %s", path);
            return;
        }
 
	count = backtrace( tracePtrs, 100 );
	backtrace_symbols_fd( tracePtrs, count, fd );
        close(fd);

	funcNames = backtrace_symbols( tracePtrs, count );

	if ( funcNames ) {
		// Print the stack trace
		for( i = 0; i < count; i++ )
		   printf("%s\n", funcNames[i] );

		// Free the string pointers
		free( funcNames );
	}
#endif
#endif
}

#if defined(_ANSC_LINUX)
static void daemonize(void) {
#ifndef  _DEBUG
	int fd;
#endif
	switch (fork()) {
	case 0:
		break;
	case -1:
		// Error
		AnscTrace("Error daemonizing (fork)! %d - %s\n", errno, strerror(
				errno));
		exit(0);
		break;
	default:
		_exit(0);
	}

	if (setsid() < 	0) {
		AnscTrace("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno));
		exit(0);
	}

#ifndef  _DEBUG

	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif
}

void sig_handler(int sig)
{
    if ( sig == SIGINT ) {
    	signal(SIGINT, sig_handler); /* reset it to this function */
    	CcspTraceError(("SIGINT received, exiting!\n"));
#if  defined(_DEBUG)
    	_print_stack_backtrace();
#endif
	exit(0);
    }
    else if ( sig == SIGUSR1 ) {
    	signal(SIGUSR1, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
    	CcspTraceInfo(("SIGUSR2 received!\n"));
        if ( bEngaged )
        {
            if ( pPsmSysRegistry )
            {
                pPsmSysRegistry->Cancel((ANSC_HANDLE)pPsmSysRegistry);
                pPsmSysRegistry->Remove((ANSC_HANDLE)pPsmSysRegistry);
            }

            bEngaged = FALSE;

    	    CcspTraceError(("Exit!\n"));

            exit(0);
        }
    }
    else if ( sig == SIGCHLD ) {
    	signal(SIGCHLD, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
    	signal(SIGPIPE, sig_handler); /* reset it to this function */
    	CcspTraceInfo(("SIGPIPE received!\n"));
    }
    else {
    	/* get stack trace first */
    	_print_stack_backtrace();
    	CcspTraceError(("Signal %d received, exiting!\n", sig));
    	exit(0);
    }

}

static int is_core_dump_opened(void)
{
    FILE *fp;
    char path[256];
    char line[1024];
    char *start, *tok, *sp;
#define TITLE   "Max core file size"

    snprintf(path, sizeof(path), "/proc/%d/limits", getpid());
    if ((fp = fopen(path, "rb")) == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((start = strstr(line, TITLE)) == NULL)
            continue;

        start += strlen(TITLE);
        if ((tok = strtok_r(start, " \t\r\n", &sp)) == NULL)
            break;

        fclose(fp);

        if (strcmp(tok, "0") == 0)
            return 0;
        else
            return 1;
    }

    fclose(fp);
    return 0;
}
#endif



int main(int argc, char* argv[])
{
    int                             cmdChar            = 0;
    BOOL                            bRunAsDaemon       = TRUE;
    int                             idx                = 0;
    FILE                           *fd                 = NULL;
    char                            cmd[64]            = {0};

    pComponentName = CCSP_DBUS_PSM;
    
#if defined(_DEBUG) || defined(_COSA_SIM_)
    AnscSetTraceLevel(CCSP_TRACE_LEVEL_INFO);
#endif

#if  defined(_ANSC_WINDOWSNT)

    AnscStartupSocketWrapper(NULL);

    gather_info();

    cmd_dispatch('e');

    while ( cmdChar != 'q' )
    {
        cmdChar = getchar();

        cmd_dispatch(cmdChar);
    }
#elif defined(_ANSC_LINUX)

    for (idx = 1; idx < argc; idx++)
    {
        if ( (strcmp(argv[idx], "-subsys") == 0) )
        {
            AnscCopyString(g_Subsystem, argv[idx+1]);
        }
        else if ( strcmp(argv[idx], "-c" ) == 0 )
        {
            bRunAsDaemon = FALSE;
        }
    }

    if ( bRunAsDaemon )
    	daemonize();

    /*This is used for ccsp recovery manager */
    fprintf(stderr, "%s -- %d", __FUNCTION__, __LINE__);

    fd = fopen("/var/tmp/PsmSsp.pid", "w+");
    if ( !fd )
    {
        AnscTrace("Create /var/tmp/PsmSsp.pid error. \n");
        return 1;
    }
    else
    {
        sprintf(cmd, "%d", getpid());
        fputs(cmd, fd);
        fclose(fd);
    }

    if (is_core_dump_opened())
    {
        signal(SIGUSR1, sig_handler);
        CcspTraceWarning(("Core dump is opened, do not catch signal\n"));
    }
    else
    {
        CcspTraceWarning(("Core dump is NOT opened, backtrace if possible\n"));
        signal(SIGTERM, sig_handler);
        signal(SIGINT, sig_handler);
        /*signal(SIGCHLD, sig_handler);*/
        signal(SIGUSR1, sig_handler);
        signal(SIGUSR2, sig_handler);

        signal(SIGSEGV, sig_handler);
        signal(SIGBUS, sig_handler);
        signal(SIGKILL, sig_handler);
        signal(SIGFPE, sig_handler);
        signal(SIGILL, sig_handler);
        signal(SIGQUIT, sig_handler);
        signal(SIGHUP, sig_handler);
        signal(SIGPIPE, sig_handler);
    }

    gather_info();

    cmd_dispatch('e');

    if ( bRunAsDaemon ) {
		while (1)
			sleep(30);
    }
    else {
        while ( cmdChar != 'q' )
        {
            cmdChar = getchar();
            if (cmdChar < 0) 
            {
                sleep(30);
            }
            else 
            {
                cmd_dispatch(cmdChar);
            }
        }
    }
#endif

    if ( bEngaged )
    {
        if ( pPsmSysRegistry )
        {
            pPsmSysRegistry->Cancel((ANSC_HANDLE)pPsmSysRegistry);
            pPsmSysRegistry->Remove((ANSC_HANDLE)pPsmSysRegistry);
        }

        bEngaged = FALSE;
    }

    return 0;
}


int  cmd_dispatch(int  command)
{
    switch ( command )
    {
        case    'e' :

                if ( !bEngaged )
                {
                    pPsmSysRegistry = (PPSM_SYS_REGISTRY_OBJECT)PsmCreateSysRegistry(NULL, NULL, NULL);

                    if ( pPsmSysRegistry )
                    {
                        PSM_SYS_REGISTRY_PROPERTY      psmSysroProperty;

                        AnscZeroMemory(&psmSysroProperty, sizeof(PSM_SYS_REGISTRY_PROPERTY));
                        
                        AnscCopyString(psmSysroProperty.SysFilePath, PSM_DEF_XML_CONFIG_FILE_PATH);
                        AnscCopyString(psmSysroProperty.DefFileName, PSM_DEF_XML_CONFIG_FILE_NAME);
                        AnscCopyString(psmSysroProperty.CurFileName, PSM_CUR_XML_CONFIG_FILE_NAME);
                        AnscCopyString(psmSysroProperty.BakFileName, PSM_BAK_XML_CONFIG_FILE_NAME);
                        AnscCopyString(psmSysroProperty.TmpFileName, PSM_TMP_XML_CONFIG_FILE_NAME);
                            
                        pPsmSysRegistry->SetProperty((ANSC_HANDLE)pPsmSysRegistry, (ANSC_HANDLE)&psmSysroProperty);

#ifdef USE_PLATFORM_SPECIFIC_HAL
                        cfm_ifo.InterfaceId   = PSM_CFM_INTERFACE_ID;
                        cfm_ifo.hOwnerContext = (ANSC_HANDLE)pPsmSysRegistry;
                        cfm_ifo.Size          = sizeof(PSM_CFM_INTERFACE);

                        cfm_ifo.ReadCurConfig = ssp_CfmReadCurConfig;
                        cfm_ifo.ReadDefConfig = ssp_CfmReadDefConfig;
                        cfm_ifo.SaveCurConfig = ssp_CfmSaveCurConfig;
                        cfm_ifo.UpdateConfigs = ssp_CfmUpdateConfigs;

                        if ( pPsmSysRegistry->hPsmCfmIf )
                        {
                            AnscFreeMemory(pPsmSysRegistry->hPsmCfmIf);
                        }

                        pPsmSysRegistry->hPsmCfmIf = (ANSC_HANDLE)&cfm_ifo;
#endif

                        pPsmSysRegistry->Engage     ((ANSC_HANDLE)pPsmSysRegistry);

                        PsmDbusInit();

                        bEngaged = TRUE;

                        CcspTraceInfo(("PSM started ...\n"));
                    }
                    else
                    {
                        CcspTraceError(("Create PSM Failed ...\n"));
                    }
                }

                break;

        case    'c' :

                if ( bEngaged )
                {
                    CcspTraceInfo(("PSM is being unloaded ...\n"));

                    if ( bus_handle != NULL )
                    {
                        CCSP_Message_Bus_Exit(bus_handle);
                    }

                    if ( pPsmSysRegistry )
                    {
#ifdef USE_PLATFORM_SPECIFIC_HAL
                        if ( pPsmSysRegistry->hPsmCfmIf )
                        {
                            pPsmSysRegistry->hPsmCfmIf = (ANSC_HANDLE)NULL;
                        }
#endif
                        pPsmSysRegistry->Cancel((ANSC_HANDLE)pPsmSysRegistry);
                        pPsmSysRegistry->Remove((ANSC_HANDLE)pPsmSysRegistry);
                    }


                    bEngaged = FALSE;

                    CcspTraceInfo(("PSM has been unloaded.\n"));
                }

                break;

        default :

                break;
    }

    return  0;
}


int  gather_info()
{
    AnscTrace("\n\n");
    AnscTrace("        ***************************************************************\n");
    AnscTrace("        ***                                                         ***\n");
    AnscTrace("        ***            PSM Testing App - Simulation                 ***\n");
    AnscTrace("        ***           Common Component Service Platform             ***\n");
    AnscTrace("        ***                                                         ***\n");
    AnscTrace("        ***        Cisco System  , Inc., All Rights Reserved        ***\n");
    AnscTrace("        ***                         2011                            ***\n");
    AnscTrace("        ***                                                         ***\n");
    AnscTrace("        ***************************************************************\n");
    AnscTrace("\n\n");

    return  0;
}


