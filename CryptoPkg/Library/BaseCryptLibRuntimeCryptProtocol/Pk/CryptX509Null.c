/** @file
  X.509 Certificate Handler Wrapper Implementation which does not provide
  real capabilities.

Copyright (c) 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalCryptLib.h"

/**
  Construct a X509 object from DER-encoded certificate data.

  Return FALSE to indicate this interface is not supported.

  @param[in]  Cert            Pointer to the DER-encoded certificate data.
  @param[in]  CertSize        The size of certificate data in bytes.
  @param[out] SingleX509Cert  The generated X509 object.

  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
X509ConstructCertificate (
  IN   CONST UINT8  *Cert,
  IN   UINTN        CertSize,
  OUT  UINT8        **SingleX509Cert
  )
{
  ASSERT (FALSE);
  return FALSE;
}

/**
  Construct a X509 stack object from a list of DER-encoded certificate data.

  Return FALSE to indicate this interface is not supported.

  @param[in, out]  X509Stack  On input, pointer to an existing X509 stack object.
                              On output, pointer to the X509 stack object with new
                              inserted X509 certificate.
  @param           ...        A list of DER-encoded single certificate data followed
                              by certificate size. A NULL terminates the list. The
                              pairs are the arguments to X509ConstructCertificate().
                                 
  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
X509ConstructCertificateStack (
  IN OUT  UINT8  **X509Stack,
  ...  
  )
{
  ASSERT (FALSE);
  return FALSE;
}

/**
  Release the specified X509 object.

  If the interface is not supported, then ASSERT().

  @param[in]  X509Cert  Pointer to the X509 object to be released.

**/
VOID
EFIAPI
X509Free (
  IN  VOID  *X509Cert
  )
{ 
  ASSERT (FALSE);
}

/**
  Release the specified X509 stack object.

  If the interface is not supported, then ASSERT().

  @param[in]  X509Stack  Pointer to the X509 stack object to be released.

**/
VOID
EFIAPI
X509StackFree (
  IN  VOID  *X509Stack
  )
{
  ASSERT (FALSE);
}

/**
  Retrieve the subject bytes from one X.509 certificate.

  Return FALSE to indicate this interface is not supported.

  @param[in]      Cert         Pointer to the DER-encoded X509 certificate.
  @param[in]      CertSize     Size of the X509 certificate in bytes.
  @param[out]     CertSubject  Pointer to the retrieved certificate subject bytes.
  @param[in, out] SubjectSize  The size in bytes of the CertSubject buffer on input,
                               and the size of buffer returned CertSubject on output.


  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
X509GetSubjectName (
  IN      CONST UINT8  *Cert,
  IN      UINTN        CertSize,
  OUT     UINT8        *CertSubject,
  IN OUT  UINTN        *SubjectSize
  )
{
  ASSERT (FALSE);
  return FALSE;
}

/**
  Retrieve the RSA Public Key from one DER-encoded X509 certificate.

  Return FALSE to indicate this interface is not supported.

  @param[in]  Cert         Pointer to the DER-encoded X509 certificate.
  @param[in]  CertSize     Size of the X509 certificate in bytes.
  @param[out] RsaContext   Pointer to new-generated RSA context which contain the retrieved
                           RSA public key component. Use RsaFree() function to free the
                           resource.

  @retval FALSE  This interface is not supported.

**/
BOOLEAN
EFIAPI
RsaGetPublicKeyFromX509 (
  IN   CONST UINT8  *Cert,
  IN   UINTN        CertSize,
  OUT  VOID         **RsaContext
  )
{
  ASSERT (FALSE);
  return FALSE;
}

/**
  Verify one X509 certificate was issued by the trusted CA.

  Return FALSE to indicate this interface is not supported.

  @param[in]      Cert         Pointer to the DER-encoded X509 certificate to be verified.
  @param[in]      CertSize     Size of the X509 certificate in bytes.
  @param[in]      CACert       Pointer to the DER-encoded trusted CA certificate.
  @param[in]      CACertSize   Size of the CA Certificate in bytes.

  @retval FALSE  This interface is not supported.
 
**/
BOOLEAN
EFIAPI
X509VerifyCert (
  IN  CONST UINT8  *Cert,
  IN  UINTN        CertSize,
  IN  CONST UINT8  *CACert,
  IN  UINTN        CACertSize
  )
{
  ASSERT (FALSE);
  return FALSE;
}
