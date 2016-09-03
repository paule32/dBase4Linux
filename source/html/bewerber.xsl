<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="de"
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      xsi:schemaLocation="http://www.w3.org/1999/xhtml
                          http://www.w3.org/MarkUp/SCHEMA/xhtml11.xsd"
>
<head>
  <title>Bewerbung - Jens Kallup</title>
  <link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body>
  <xsl:apply-templates />
</body></html>
</xsl:template>

<xsl:template match="bewerber">
  hallo
  <xsl:apply-templates select="contact"/>
</xsl:template>

</xsl:stylesheet>
