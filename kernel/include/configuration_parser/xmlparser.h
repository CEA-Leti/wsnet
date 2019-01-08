/**
 *  \file   xmlparser.h
 *  \brief  XML parser declarations
 *  \author Loic Lemaitre
 *  \date   2010
 **/
#ifndef __xmlparser__
#define __xmlparser__

#include <string.h>
#include <assert.h>
#include <math.h>

#include <wsnet_build_info.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlschemas.h>

#include <kernel/include/options.h>

#include <kernel/include/data_structures/list/list.h>
#include <kernel/include/data_structures/circular_array/circ_array.h>
#include <kernel/include/data_structures/sliding_window/sliding_window.h>

#include <kernel/include/definitions/class.h>
#include <kernel/include/definitions/medium.h>
#include <kernel/include/definitions/environment.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/group.h>
#include <kernel/include/definitions/simulation.h>


/* ************************************************** */
/* ************************************************** */
#ifndef DBG_PRINT
#define DBG_PRINT(x...) fprintf(stderr,x)
#endif

//#define DBG_MSG_CLASS        /* config class debug messages */
//#define DBG_MSG_MEDIUM       /* config medium debug messages */
//#define DBG_MSG_ENVIRONMENT  /* config environment debug messages */
//#define DBG_MSG_NODEARCH     /* config node architecture debug messages */
//#define DBG_MSG_SIMULATION   /* config simulation debug messages */

#ifdef DBG_MSG_CLASS
#define DBG_CLASS(x...) DBG_PRINT(x)
#else
#define DBG_CLASS(x...) do { } while (0)
#endif

#ifdef DBG_MSG_MEDIUM
#define DBG_MEDIUM(x...) DBG_PRINT(x)
#else
#define DBG_MEDIUM(x...) do { } while (0)
#endif

#ifdef DBG_MSG_ENVIRONMENT
#define DBG_ENVIRONMENT(x...) DBG_PRINT(x)
#else
#define DBG_ENVIRONMENT(x...) do { } while (0)
#endif

#ifdef DBG_MSG_NODEARCH
#define DBG_NODEARCH(x...) DBG_PRINT(x)
#else
#define DBG_NODEARCH(x...) do { } while (0)
#endif

#ifdef DBG_MSG_GROUPARCH
#define DBG_GROUPARCH(x...) DBG_PRINT(x)
#else
#define DBG_GROUPARCH(x...) do { } while (0)
#endif

#ifdef DBG_MSG_SIMULATION
#define DBG_SIMULATION(x...) DBG_PRINT(x)
#else
#define DBG_SIMULATION(x...) do { } while (0)
#endif


/* ************************************************** */
/* ************************************************** */
/* XML keywords (tag, class, namespace, xpath) */

/*************/
/* Namespace */
/*************/
#define XML_NS_ID		"w"
#define XML_NS_URL		"http://www.cea.fr"


/************/
/* Elements */
/************/
#define XML_E_WORLDSENS          "worldsens"

#define XML_E_MAC                "mac"
#define XML_E_TRANSCEIVER        "transceiver"
#define XML_E_ROUTING            "routing"
#define XML_E_APPLICATION        "application"
#define XML_E_INTERFERENCES      "interferences"
#define XML_E_INTERMODULATION    "intermodulation"
#define XML_E_MODULATION         "modulation"
#define XML_E_ENERGY             "energy"
#define XML_E_SPECTRUM           "spectrum"
#define XML_E_PATHLOSS           "pathloss"
#define XML_E_SHADOWING          "shadowing"
#define XML_E_FADING             "fading"
#define XML_E_NOISE              "noise"
#define XML_E_MONITOR            "monitor"
#define XML_E_MOBILITY           "mobility"
#define XML_E_MAP                "map"
#define XML_E_LINK               "link"
#define XML_E_GLOBAL_MAP         "global_map"
#define XML_E_INTERFACE          "interface"
#define XML_E_PHY                "phy"
#define XML_E_CODING             "coding"
#define XML_E_ERROR              "error"
#define XML_E_SIGNAL_TRACKER     "signal_tracker"
#define XML_E_INTERFERENCE       "interference"
#define XML_E_MODULATOR       	 "modulator"
#define XML_E_PHYSICAL           "physical"
#define XML_E_SENSOR             "sensor"

#define XML_E_MEDIUM             "medium"
#define XML_E_ENVIRONMENT        "environment"
#define XML_E_NODE_ARCHITECTURE  "node_architecture"
#define XML_E_SIMULATION         "simulation"

#define XML_E_CLASS_PARAMETERS   "class_parameters"
#define XML_E_PARAMETERS         "parameters"
#define XML_E_DEFAULT_PARAMETERS "default_parameters"
#define XML_E_PARAM              "param"
#define XML_E_IMPLEMENTATION     "implementation"
#define XML_E_NODE_TYPES		 "node_types"
#define XML_E_TYPE_FAMILY		 "family"
#define XML_E_TYPE			     "type"
#define XML_E_RUBY               "ruby"
#define XML_E_PYTHON             "python"
#define XML_E_CXX                "cxx"
#define XML_E_C                  "c"
#define XML_E_UP                 "up"
#define XML_E_DOWN               "down"
#define XML_E_NODE               "node"
#define XML_E_WSIM               "wsim"

#define XML_E_GROUP_ARCHITECTURE "group_architecture"
#define XML_E_LEADER             "leader"
#define XML_E_MEMBER             "member"

/**************/
/* Attributes */
/**************/
#define XML_A_DURATION           "duration"
#define XML_A_NODES              "nodes"
#define XML_A_GROUPS             "groups"
#define XML_A_X                  "x"
#define XML_A_Y                  "y"
#define XML_A_Z                  "z"
#define XML_A_NAME               "name"
#define XML_A_PROPAGATION_RANGE  "propagation_range"
#define XML_A_DEFAULT            "default"
#define XML_A_BIRTH              "birth"
#define XML_A_ID                 "id"
#define XML_A_ARCHITECTURE       "architecture"
#define XML_A_KEY                "key"
#define XML_A_VALUE              "value"
#define XML_A_CLASS              "class"
#define XML_A_MEDIUM             "medium"
#define XML_A_ENVIRONMENT        "environment"
#define XML_A_NBR_NODES          "nbr_nodes"
#define XML_A_TYPE			     "type"


/*********************/
/* Attribute content */
/*********************/
#define XML_C_LIBRARY            "library"
#define XML_C_WSIM               "wsim"


/*********/
/* XPath */
/*********/
#define XML_X(x) "/" XML_NS_ID ":" XML_E_WORLDSENS	\
                 "/" XML_NS_ID ":" x

#define XML_X_CLASSES   XML_X(XML_E_MAC)             "|"  \
				                XML_X(XML_E_TRANSCEIVER)     "|"  \
				                XML_X(XML_E_ROUTING)         "|"  \
                        XML_X(XML_E_APPLICATION)     "|"  \
                        XML_X(XML_E_INTERFERENCES)   "|"  \
                        XML_X(XML_E_INTERMODULATION) "|"  \
                        XML_X(XML_E_MODULATION)      "|"  \
                        XML_X(XML_E_ENERGY)          "|"  \
                        XML_X(XML_E_SPECTRUM)        "|"  \
                        XML_X(XML_E_PATHLOSS)        "|"  \
                        XML_X(XML_E_SHADOWING)       "|"  \
                        XML_X(XML_E_FADING)          "|"  \
                        XML_X(XML_E_NOISE)           "|"  \
                        XML_X(XML_E_MONITOR)         "|"  \
                        XML_X(XML_E_MOBILITY)        "|"  \
                        XML_X(XML_E_MAP)             "|"  \
                        XML_X(XML_E_LINK)            "|"  \
                        XML_X(XML_E_GLOBAL_MAP)      "|"  \
                        XML_X(XML_E_SENSOR)          "|"  \
                        XML_X(XML_E_INTERFACE)       "|"  \
                        XML_X(XML_E_PHY)             "|"  \
                        XML_X(XML_E_SIGNAL_TRACKER)  "|"  \
                        XML_X(XML_E_ERROR)           "|"  \
                        XML_X(XML_E_CODING)          "|"  \
                        XML_X(XML_E_INTERFERENCE)    "|"  \
                        XML_X(XML_E_MODULATOR)	     "|"  \
                        XML_X(XML_E_PHYSICAL)
#define XML_X_MEDIUM            XML_X(XML_E_MEDIUM)
#define XML_X_ENVIRONMENT       XML_X(XML_E_ENVIRONMENT)
#define XML_X_NODE_ARCHITECTURE XML_X(XML_E_NODE_ARCHITECTURE)
#define XML_X_GROUP_ARCHITECTURE XML_X(XML_E_GROUP_ARCHITECTURE)
#define XML_X_SIMULATION        XML_X(XML_E_SIMULATION)

/* ************************************************** */
/* ************************************************** */
typedef struct _dflt_param {
    classid_t       classid;
    nodearchid_t    nodearchid;
    nodeid_t        nodeid;
    mediumid_t      mediumid;
    environmentid_t environmentid;
    void * params;
} dflt_param_t;

extern void *dflt_params;

extern gchar **user_path_list;
extern gchar **sys_path_list;

/* ************************************************** */
/* ************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void config_set_configfile    (char *c);
void config_set_schemafile    (char *c);
void config_set_sys_modulesdir(char *c);
int  do_configuration         (void);

dflt_param_t *get_class_params(nodeid_t node, classid_t class, nodearchid_t nodearch,
		       mediumid_t medium, environmentid_t environment);

/**
 * \brief Search for the "attr_name", in the xml node "nd1".
 * \param nd1 the xml node pointer, and attr_name the attribute string to find.
 * \return Return a pointer on the content of the attribute.
 **/
char *get_xml_attr_content(xmlNodePtr nd1, const char *attr_name);

/**
 * \brief Duplicate params1 list into a params2 list.
 * \param params1 a void pointer on a list list, params2 a void pointer.
 * \return Return 0 if case of success, -1 else.
 **/
int  duplicate_class_params(void *params1, void *params2);

/**
 * \brief Search for the param key in the params list.
 * \param params a void pointer on a list list, key the param key to find.
 * \return Return NULL if not found, a param pointer in case of success .
 **/
param_t *find_class_param_key(void *params, char *key);

#ifdef __cplusplus
}
#endif

#endif //__xmlparser__
