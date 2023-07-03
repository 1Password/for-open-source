###############################################
Customize hardware signature generators 
###############################################

Change the hardware identification strategy
*************************************************

Included with the library there are three hardware identification strategies: `IP_ADDRESS`, `STRATEGY_ETHERNET` (mac address) and 
`STRATEGY_DISK` (partition serial number). If you want to change the strategy that is used to generate the default identifier:
  
  - locate the file `licensecc_properties.h`` (usually in ``projects/<$project_name>/include/licensecc/<$project_name>``
  - you can change the order of the strategies in the following code block (the strategies will be tried in sequence until the first one succeeds):

.. code-block:: c
#define LCC_BARE_TO_METAL_STRATEGIES { STRATEGY_DISK, STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_VM_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_LXC_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_DOCKER_STRATEGIES { STRATEGY_NONE }
#define LCC_CLOUD_STRATEGIES { STRATEGY_NONE }

Implement your own hardware signature generator 
*************************************************

.. doxygenclass:: license::hw_identifier::IdentificationStrategy
