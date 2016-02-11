#include "atomic_lock.h"

__declspec(naked) int __sync_fetch_and_sub(int *p, int n) {
  //4013e0:	55                   	push   %ebp
  //4013e1:	89 e5                	mov    %esp,%ebp
  //4013e3:	8b 55 0c             	mov    0xc(%ebp),%edx
  //4013e6:	8b 45 08             	mov    0x8(%ebp),%eax
  //4013e9:	f7 da                	neg    %edx
  //4013eb:	f0 0f c1 10          	lock xadd %edx,(%eax)
  //4013ef:	89 d0                	mov    %edx,%eax
  //4013f1:	5d                   	pop    %ebp
  //4013f2:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		mov		edx,[ebp + 0xc]
		mov		eax,[ebp + 0x8]
		neg		edx
		lock	xadd [eax],edx
		mov		edx,eax
		pop		ebp
		ret
	}
}

__declspec(naked) int __sync_fetch_and_add(int *p, int n) {
  //4013f3:	55                   	push   %ebp
  //4013f4:	89 e5                	mov    %esp,%ebp
  //4013f6:	8b 55 0c             	mov    0xc(%ebp),%edx
  //4013f9:	8b 45 08             	mov    0x8(%ebp),%eax
  //4013fc:	f0 0f c1 10          	lock xadd %edx,(%eax)
  //401400:	89 d0                	mov    %edx,%eax
  //401402:	5d                   	pop    %ebp
  //401403:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		mov		edx,[ebp + 0xc]
		mov		eax,[ebp + 0x8]
		lock	xadd [eax],edx
		mov		eax,edx
		pop		ebp
		ret
	}
}

__declspec(naked) int __sync_add_and_fetch(int *p, int n) {
  //401404:	55                   	push   %ebp
  //401405:	89 e5                	mov    %esp,%ebp
  //401407:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  //40140a:	8b 55 08             	mov    0x8(%ebp),%edx
  //40140d:	89 c8                	mov    %ecx,%eax
  //40140f:	f0 0f c1 02          	lock xadd %eax,(%edx)
  //401413:	01 c8                	add    %ecx,%eax
  //401415:	5d                   	pop    %ebp
  //401416:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		mov		ecx,[ebp + 0xc]
		mov		edx,[ebp + 0x8]
		mov		eax,ecx
		lock	xadd [edx],eax
		add		eax,ecx
		pop		ebp
		ret
	}
}

__declspec(naked) int __sync_sub_and_fetch(int *p, int n) {
  //401417:	55                   	push   %ebp
  //401418:	89 e5                	mov    %esp,%ebp
  //40141a:	8b 45 0c             	mov    0xc(%ebp),%eax
  //40141d:	8b 55 08             	mov    0x8(%ebp),%edx
  //401420:	f7 d8                	neg    %eax
  //401422:	89 c1                	mov    %eax,%ecx
  //401424:	89 c8                	mov    %ecx,%eax
  //401426:	f0 0f c1 02          	lock xadd %eax,(%edx)
  //40142a:	01 c8                	add    %ecx,%eax
  //40142c:	5d                   	pop    %ebp
  //40142d:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		mov		eax,[ebp + 0xc]
		mov		edx,[ebp + 0x8]
		neg		eax
		mov		ecx,eax
		mov		eax,ecx
		lock	xadd [edx],eax
		add		eax,ecx
		pop		ebp
		ret
	}
}

__declspec(naked) int __sync_lock_test_and_set(int *p, int n) {
	__asm {
		push	ebp
		mov		ebp,esp
		mov		edx,[ebp + 0xc]
		mov		eax,[ebp + 0x8]
		xchg	[eax],edx
		mov		eax,edx
		pop		ebp
		ret
	}
}

__declspec(naked) void __sync_lock_release(int *p) {
	__asm {
		push	ebp
		mov		ebp,esp
		mov		eax,[ebp + 0x8]
		mov		edx,0
		mov		[eax],edx
		nop
		pop		ebp
		ret
	}
}

__declspec(naked) void __sync_synchronize() {
  //401449:	55                   	push   %ebp
  //40144a:	89 e5                	mov    %esp,%ebp
  //40144c:	f0 83 0c 24 00       	lock orl $0x0,(%esp)
  //401451:	5d                   	pop    %ebp
  //401452:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		lock	or [esp],0
		pop		ebp
		ret
	}
}

__declspec(naked) char __sync_bool_compare_and_swap(int *p, int value, int compare) {
  //401453:	55                   	push   %ebp
  //401454:	89 e5                	mov    %esp,%ebp
  //401456:	8b 4d 10             	mov    0x10(%ebp),%ecx
  //401459:	8b 45 0c             	mov    0xc(%ebp),%eax
  //40145c:	8b 55 08             	mov    0x8(%ebp),%edx
  //40145f:	f0 0f b1 0a          	lock cmpxchg %ecx,(%edx)
  //401463:	0f 94 c0             	sete   %al
  //401466:	0f b6 c0             	movzbl %al,%eax
  //401469:	5d                   	pop    %ebp
  //40146a:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		mov		ecx,[compare]
		mov		eax,[value]
		mov		edx,[p]
		lock	cmpxchg [edx],ecx
		sete	al
		movzx	eax,al
		pop		ebp
		ret
	}
}

__declspec(naked) int __sync_and_and_fetch(int *p, int n) {
  //40146b:	55                   	push   %ebp
  //40146c:	89 e5                	mov    %esp,%ebp
  //40146e:	56                   	push   %esi
  //40146f:	53                   	push   %ebx
  //401470:	8b 75 0c             	mov    0xc(%ebp),%esi
  //401473:	8b 55 08             	mov    0x8(%ebp),%edx
  //401476:	8b 02                	mov    (%edx),%eax
  //401478:	89 c1                	mov    %eax,%ecx
  //40147a:	21 f1                	and    %esi,%ecx
  //40147c:	89 cb                	mov    %ecx,%ebx
  //40147e:	f0 0f b1 0a          	lock cmpxchg %ecx,(%edx)
  //401482:	0f 94 c1             	sete   %cl
  //401485:	84 c9                	test   %cl,%cl
  //401487:	74 ef                	je     401478 <_sync_and_and_fetch+0xd>
  //401489:	89 d8                	mov    %ebx,%eax
  //40148b:	5b                   	pop    %ebx
  //40148c:	5e                   	pop    %esi
  //40148d:	5d                   	pop    %ebp
  //40148e:	c3                   	ret    
	__asm {
		push	ebp
		mov		ebp,esp
		push	esi
		push	ebx
		mov		esi,[ebp + 0xc]
		mov		edx,[ebp + 0x8]
		mov		eax,[edx]
retry:
		mov		ecx,eax
		and		ecx,esi
		mov		ebx,ecx
		lock	cmpxchg [edx],ecx
		sete	cl
		test	cl,cl
		je		retry
		mov		eax,ebx
		pop		ebx
		pop		esi
		pop		ebp
		ret
	}
}
