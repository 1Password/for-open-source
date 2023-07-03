#######################################
Extension points
#######################################

The version 2.0 of the library comes with clear API extension and customization points. 

Tweak hardware signature generator
***************************************

If the provided hardware signatures don't behave well for your customers, or you want to change the default
way the library generates the pc identifier you can have a look at the following section.

First of all be sure to read about the standard behavior of :c:func:`identify_pc` here:

.. toctree::

   hardware_identifiers


Change the hardware identification strategy
============================================

Included with the library there are three hardware identification strategies: `IP_ADDRESS`, `STRATEGY_ETHERNET` (mac address) and 
`STRATEGY_DISK` (partition serial number). If you want to change the preferred one:
  
  - locate the file `licensecc_properties.h`` (usually in ``projects/<$project_name>/include/licensecc/<$project_name>``
  - you can change the order of the strategies in the following code block (the strategies will be tried in sequence until the first one succeeds):


.. code-block:: c
#define LCC_BARE_TO_METAL_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_VM_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_LXC_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_DOCKER_STRATEGIES { STRATEGY_NONE }
#define LCC_CLOUD_STRATEGIES { STRATEGY_NONE }

   
Custom license locator 
***************************************
Your software struggle to find the license file ? you can implement your own way to find it. For instance if you want 
to download the license from a remote server, you could implement the logic here. 
 
.. TODO::
   
   this section need to be completed
