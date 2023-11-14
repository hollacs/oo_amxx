class B1:
	def mth(self):
		print('B1 mth')
		
class B2:
	def mth(self):
		print('B2 mth')

class Base(B1,B2):
	def mth(self):
		super().mth()
		print('Base mth')

class Parent1(Base):
	def mth(self):
		super().mth()
		print('P1 mth')

class Parent2(Base):
	def mth(self):
		super().mth()
		print('P2 mth')

class Child(Parent1, Parent2):
	def mth(self):
		super().mth()
		print('Child mth')

class Final(Child): # Child1 Parent1 Parent2 Child2 Parent2 Parent3 Base
	def mth(self):
		super().mth()
		print('Final mth')

o = Final()
o.mth()