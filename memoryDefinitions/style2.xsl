<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:fn="http://www.w3.org/2006/xpath-functions" xmlns:xf="http://www.w3.org/2002/xforms"
	xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" encoding="utf-8" indent="yes"
		method="xml"/>
	<xsl:template match="/" priority="97">
		<xsl:apply-templates/>
	</xsl:template>
	<xsl:template match="dramspec" priority="100">
		<html>
			<head>
				<link href="style.css" media="screen" rel="stylesheet" type="text/css" />
				<script language="Javascript">
				function getFN(context)
				{
				return context.nextNode.url;		
				}
				</script>
				<style type="text/css">
				@namespace xf url("http://www.w3.org/2002/xforms");
				xf|input
				{
				text-align:center;
				display: table-row;
				line-height: 2em;
				}
				xf|label, xf|output
				{
				display: table-cell;
				text-align: right;
				font-family: Bitstream Vera Sans,Arial, Helvetica, sans-serif;
				font-weight: bold;
				font-size: small;
				padding: 5px;
				width: 150px;
				}		
				*:invalid { background-color: red; }						
				.one { background-color:#E5E4E4; }
				.other { background-color:#FCF8F6; }
				.heading { background-color:#F0D0CC; }
				</style>
				<meta content="application/xml; charset=utf8" http-equiv="Content-Type"/>
				<xf:model>
					<xsl:attribute name="id">dramspec</xsl:attribute>
					<xsl:attribute name="schema">schema2.xsd</xsl:attribute>
					<xf:instance>
						<!--xsl:attribute name="src">documentURI()</xsl:attribute-->
					</xf:instance>
					<xf:submission id="save-to-local-file" method="put">
						<xsl:attribute name="action">documentURI()</xsl:attribute>
					</xf:submission>
				</xf:model>
				<title> DRAM Values <xsl:value-of select="@type"/>
				</title>
			</head>
			<body>
				<xsl:apply-templates mode="headings"/>
				<xf:submit submission="save-to-local-file">
					<xf:label>Save</xf:label>
					<xf:hint>Save to local file</xf:hint>
				</xf:submit>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="*" mode="headings" priority="99">
		<xf:output>
			<xsl:attribute name="class">heading</xsl:attribute>
			<xsl:value-of select="name()"/>
		</xf:output>
		<xf:group model="dramspec">
			<xsl:attribute name="ref">/dramspec/<xsl:value-of select="name()"/></xsl:attribute>
			<xsl:apply-templates mode="parameters" select="*"/>
		</xf:group>
	</xsl:template>
	<xsl:template match="*" mode="parameters" priority="98">
		<xf:input>
			<xsl:attribute name="ref">
				<xsl:value-of select="name()"/>
			</xsl:attribute>
			<xsl:attribute name="class">
				<xsl:choose>
					<xsl:when test="position() mod 2 = 0">other</xsl:when>
					<xsl:otherwise>one</xsl:otherwise>
				</xsl:choose>
			</xsl:attribute>
			<xf:label>
				<acronym>
					<xsl:attribute name="title">
						<xsl:value-of select="name()"/>
					</xsl:attribute>
					<xsl:value-of select="name()"/>
				</acronym>
			</xf:label>
		</xf:input>
	</xsl:template>
</xsl:stylesheet>
