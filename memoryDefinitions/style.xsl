<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xhtml="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
	<xsl:template match="/" priority="97">
		<xsl:apply-templates/>
	</xsl:template>
	<xsl:template match="dramspec" priority="100">
		<head>
			<link href="style.css" rel="stylesheet" type="text/css" media="screen"/>
			<title>
        DRAM Values <xsl:value-of select="@type"/>
			</title>
		</head>
		<body>
		<div id="content-container">
			<form action="post">
				<table class="content" style="width: 300px;" align="center">
					<caption>DRAM Parameters</caption>
					<tbody>
						<tr>
							<th rowspan="1">Parameter</th>
							<th rowspan="1">Value</th>
						</tr>
						<xsl:apply-templates mode="headings"/>
					</tbody>
				</table>
				<input type="submit"><xsl:attribute name="value">Save File</xsl:attribute></input>
			</form>
			</div>
		</body>
	</xsl:template>
	<xsl:template match="*" mode="headings" priority="99">
		<tr>
			<xsl:attribute name="class">heading</xsl:attribute>
			<td align="center" colspan="2">
				<xsl:value-of select="name()"/>
			</td>
		</tr>
		<xsl:apply-templates select="*" mode="parameters"/>
	</xsl:template>
	<xsl:template match="*" mode="parameters" priority="98">
		<tr>
			<xsl:choose>
				<xsl:when test="position() mod 2 = 0">
					<xsl:attribute name="class">other</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">one</xsl:attribute>
				</xsl:otherwise>
			</xsl:choose>
			<td align="center">
				<xsl:value-of select="name()"/>
			</td>
			<xsl:variable name="current">
				<xsl:value-of select="name()"/>
			</xsl:variable>
			<td align="center">
				<input type="text" align="middle">
				<xsl:attribute name="size"><xsl:value-of select="string-length(node())"/></xsl:attribute>
					<xsl:attribute name="value"><xsl:value-of select="node()"/></xsl:attribute>
				</input>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
