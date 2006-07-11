class addresses
{
public:
	unsigned int		virt_addr;	
	unsigned int		phys_addr;
	unsigned int		chan_id;		/* logical channel id */
	unsigned int		rank_id;		/* device id */
	unsigned int		bank_id;
	unsigned int		row_id;
	unsigned int		col_id;			/* column address */
	addresses();
	//friend ostream &operator<<(ostream &, const addresses &);
	//ostream &operator<<(ostream &, const addresses &);
};