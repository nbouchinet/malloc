/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbouchin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/13 09:03:40 by nbouchin          #+#    #+#             */
/*   Updated: 2018/06/25 16:16:50 by nbouchin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft_malloc.h"

t_zone g_zone[3] = {{0, NULL},
	{0, NULL},
	{0, NULL}};

int		no_place(t_block *p, size_t alloc_size)
{
	if (p->size >= alloc_size && p->is_free)
		return (0);
	return (1);
}

void	*new_zone(size_t index, size_t alloc_size, size_t zone_size)
{
	t_block		*p;
	t_page		*page;

	p = NULL;
	page = NULL;
	if (!g_zone[index].total_size)
	{
		g_zone[index].page = mmap(NULL, zone_size, PROT_WRITE | PROT_READ,
				MAP_ANON | MAP_PRIVATE, -1, 0);
		ft_bzero(g_zone[index].page, sizeof(t_page));
		g_zone[index].total_size = zone_size;
		g_zone[index].page->size = zone_size - sizeof(t_page *);
		g_zone[index].page->nxt = NULL;
	}
	page = g_zone[index].page;
	while (page->nxt)
		page = page->nxt;
	p = (t_block *)(page + 1);
	while (p->nxt && no_place(p, alloc_size))
		p = p->nxt;
	if ((char*)(p + 1) + alloc_size >= (char*)(page + 1) + zone_size)
	{
		page->nxt = mmap(NULL, zone_size, PROT_WRITE | PROT_READ,
				MAP_ANON | MAP_PRIVATE, -1, 0);
		g_zone[index].total_size += zone_size;
		page->nxt->size = zone_size;
		page->nxt->nxt = NULL;
		p = (t_block *)(page->nxt + 1);
	}
	p->is_free = 0;
	p->size = alloc_size;
	if (!p->nxt)
	{
		p->nxt = (t_block *)((char *)(p + 1) + alloc_size);
		p->nxt->size = (char *)page + page->size - (char *)p->nxt;
		p->nxt->is_free = 1;
	}
	return (p + 1);
}

size_t	get_offset(size_t alloc_size, int offset)
{
	return (alloc_size = (alloc_size + (offset - 1)) & ~(offset - 1));
}

void	*check_zone(size_t alloc_size)
{
	if (alloc_size >= 1 && alloc_size <= TINY)
		return (new_zone(0, get_offset(alloc_size, 16), N));
	else if (alloc_size >= TINY + 1 && alloc_size < SMALL)
		return (new_zone(1, get_offset(alloc_size, 512), M));
	else if (alloc_size >= LARGE)
		return (new_zone(2, alloc_size, alloc_size + 16));
	return(0);
}

void	*ft_malloc(size_t alloc_size)
{
	return (check_zone(alloc_size));
}
