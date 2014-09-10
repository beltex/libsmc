c-smc
=====

Apple System Management Controller (SMC) API in pure C from user space for Intel
based Macs. The API works by talking to the AppleSMC.kext (kernel
extension), the closed source driver for the SMC.

For a Swift based version see [swift-smc](https://github.com/beltex/swift-smc).


### System Management Controller

_"The System Management Controller (SMC) is an internal subsystem introduced by
Apple Inc. with the introduction of their new Intel processor based machines
in 2006. It takes over the functions of the SMU. The SMC manages thermal and
power conditions to optimize the power and airflow while keeping audible noise
to a minimum. Power consumption and temperature are monitored by the operating
system, which communicates the necessary adjustments back to the SMC. The SMC
makes the changes, slowing down or speeding up fans as necessary."_
-via Wikipedia

For more see:

- [System Management Controller](http://en.wikipedia.org/wiki/System_Management_Controller)
- [System Management Unit](http://en.wikipedia.org/wiki/System_Management_Unit)
- [Power Management Unit](http://en.wikipedia.org/wiki/Power_Management_Unit)


### Requirements

- OS X 10.6+


### References

Handy I/O Kit references:

- [iOS Hacker's Handbook](http://ca.wiley.com/WileyCDA/WileyTitle/productCd-1118204123.html)
- [Mac OS X and iOS Internals: To the Apple's Core](http://ca.wiley.com/WileyCDA/WileyTitle/productCd-1118057651.html)
- [OS X and iOS Kernel Programming](http://www.apress.com/9781430235361-4892)


### License

This project is under the **GNU General Public License v2.0**.
