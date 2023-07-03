# Backoffice (Draft) (Use Cases)

This is a very old sketch of a backoffice domain model (this documentation should be removed since it's not in the 
current plans)

## Domain

**Software Provider**
* PK ProviderId
* Email (UNIQUE)
* Password
* LicenseGenerator?? (UNIQUE)
* Library headers and compiled files (zip).

**Product**
* PK ProductId + (FK) ProviderId

**Software User**
* PK UserId + (FK) ProviderId
* Email (UNIQUE)

**License**
* PK LicenceId (AUTO)
* FK ProductId + UserId
* StartDate
* EndDate (NULL)
* HostId (NULL)


## Use Cases ...

**Software Provider Sign Up**
 \<\<inlcude\>\>
 * Generate Keys
 * Compile (provider specific) license generator and libraries bundle.

**Software Provider Sign In**

**Create New License**

Alternative scenario : 
create new User, create new Product

**List Licences**

**(Renew License)**

**(Email Expiring Licenses)**