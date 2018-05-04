	This file describes the implementation of the Communications Protocol of this Program.
	The Protocol is based on the Abstract Protocol Layout of PkmCom 
   [PkmCom Protocol Definition and APL](<https://docs.google.com/document/d/1hqRjaHQU3yXQVvzzb8121tUnnlGSVQq62ASIj7fCtBM>)

	Abstract Protocol Layout Changes:

	The following changes were to the APL of the protocol to make it viable in this case:
	
	The "PokemonData" structure is removed
	The Long String, Long Json, and Json types are not used.
	
	In addition, the "Employee" structure type is added using the following layout
<table>
	<tr>
		<th>Field</th>
		<th>Type</th>
		<th>Description</th>
	</tr>
	<tr>
		<td>Name</td>
		<td>String</td>
		<td>The name of the employee (first and last name)</td>
	</tr>
	<tr>
		<td>Id</td>
		<td>UUID</td>
		<td>The Id of the employee</td>
	</tr>
	<tr>
		<td>PublicKey</td>
		<td>Byte Array (128 bytes)</td>
		<td>The public Key of the employee</td>
	</tr>
	<tr>
		<td>Status</td>
		<td>Byte Enum</td>
		<td>The Status of the employee</td>
	</tr>
	<tr>
		<td>Permissions</td>
		<td>Byte Bitflag</td>
		<td>The permissions of the employee</td>
	</tr>
</table>

	The Status Enum is defined as follows:
<table>
	<tr>
		<th>Value</th>
		<th>Description</th>
	</tr>
	<tr>
		<td>0</td>
		<td>Offline</td>
	</tr>
	<tr>
		<td>1</td>
		<td>Busy</td>
	</tr>
	<tr>
		<td>2</td>
		<td>Online</td>
	</tr>
</table>

	And the Permissions Bitflag is defined as follows:
<table>
	<tr>
		<th>Bit</th>
		<th>Description</th>
	</tr>
	<tr>
		<td>0x01</td>
		<td>Authenticate</td>
	</tr>
	<tr>
		<td>0x02</td>
		<td>Modify Employees</td>
	</tr>
	<tr>
		<td>0x04</td>
		<td>Delete Employees</td>
	</tr>
	<tr>
		<td>0x08</td>
		<td>Add Employees</td>
	</tr>
	<tr>
		<td>0x10</td>
		<td>Administrator/Superuser</td>
	</tr>
</table>

	The hashcode of the Employee Type is given by taking the hashsum of the hashcode of each field in order
	(hashsum is given by adding together the values, multiplying the previous total by 31 before adding the next value, starting at 0)

	The Protocol:

[Protocol Layout](https://docs.google.com/document/d/1j5r-z6lJQ3zyhib1KpUsrSG0lSR8E20drOiNftBqHmM/edit?usp=sharing)
		
	
	


