<p align="center">
  <img alt="logo" src="docs/main_banner.webp">
</p>

[![HitCount](https://hits.dwyl.com/marcelschreiner/loxone-hue-picoc.svg?style=flat)](http://hits.dwyl.com/marcelschreiner/loxone-hue-picoc)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=bugs)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=marcelschreiner_loxone-hue-picoc&metric=sqale_index)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)


[![BuyMeACoffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/marcelschreiner)

# loxone-hue-picoc
A PicoC script to control Philips Hue lights from a Loxone Miniserver. Using this solution it is not necessary to rely on a thirdparty bridge to translate between the Miniserver and the Hue Bridge.

> [!NOTE]
> Unfortunately this PicoC code can only send commands to the Hue system. Is is however not possible to receive events from the Hue system. Have a look at the [repo hue-to-loxone](https://github.com/marcelschreiner/hue-to-loxone) if you also want to receive events from your Hue system.

# Getting Started
Follow these steps to get the PicoC script working:
- Generate a Philips hue API key. [This blog entry](https://www.sitebase.be/generate-phillips-hue-api-token/) describes how to do this.
- Enter the ip address and Hue API key in the PicoC script.
- Get a list of all the id's of the lights connected to your Hue bridge. Go to:
`https://<bridge-ip>/api/<api-key>/lights`. This will show a json file with all the id's.
- Enter all the id's and device types you want to control in the PicoC script.
- If you exceed the 13 devices limit, you can ether use Hue groups `https://<bridge-ip>/api/<api-key>/groups`, or you can run up to 8 PicoC scripts simultaneously on the Loxone Miniserver.
<br/><br/>

## License
Released under the [MIT License](LICENSE.md), this code is yours to command! 🚀 Modify it, tweak it, use it to your heart's content. Let's create something amazing together! 💻🌟

<br/>

[![SonarCloud](https://sonarcloud.io/images/project_badges/sonarcloud-black.svg)](https://sonarcloud.io/summary/new_code?id=marcelschreiner_loxone-hue-picoc)
