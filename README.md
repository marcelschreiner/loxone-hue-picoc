# loxone-hue-picoc

[![HitCount](https://hits.dwyl.com/marcelschreiner/loxone-hue-picoc.svg?style=flat)](http://hits.dwyl.com/marcelschreiner/loxone-hue-picoc)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=bugs)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=sqale_index)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)


A PicoC script to control Philips Hue lights from a Loxone Miniserver. Using this solution it is not necessary to rely on a thirdparty bridge to translate between the Miniserver and the Hue Bridge.

Unfortunately this PicoC code can only control send commands to the Hue system. Is is however not possible to receive events from the Hue system. For this to work https would be required (which is not available in PicoC).
Have a look at the [repo hue-to-loxone](https://github.com/marcelschreiner/hue-to-loxone) if you also want to receive events from motion sensors or hue dimmer switches.


*THIS CODE IS A WORK IN PROGRESS! NOT YET FULLY WORKING!*


## License

This script is provided under the [MIT License](LICENSE.md). Feel free to modify and use it according to your needs.

<br />

[![SonarCloud](https://sonarcloud.io/images/project_badges/sonarcloud-black.svg)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
