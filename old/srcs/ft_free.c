/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbouchin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/13 09:13:59 by nbouchin          #+#    #+#             */
/*   Updated: 2018/06/29 16:31:50 by nbouchin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft_malloc.h"
#include <stdio.h>


void		defrag(t_block **p, void *ptr)
{
	t_block		*prev;

	prev = NULL;
	while ((*p)->nxt)
	{
		if ((t_block *)((*p) + 1) == (t_block *)ptr)
		{ (*p)->is_free = 1;
			if ((*p)->nxt && (*p)->nxt->is_free)
			{
				(*p)->size += (*p)->nxt->size + sizeof(t_block);
				(*p)->nxt = (*p)->nxt->nxt;
			}
			if (prev && prev->is_free)
			{
				prev->size += (*p)->size + sizeof(t_block);
				prev->nxt = (*p)->nxt;
				(*p) = prev;
			}
			break ;
		}
		prev = (*p);
		(*p) = (*p)->nxt;
	}
}

void		delete_page(t_block *p, t_page **page, t_page **prev, int i)
{
	if ((p->size == (*page)->size - sizeof(t_block)) && (*page) == g_zone[i].page && i == 2)
	{
		if ((*page)->nxt)
			g_zone[2].page = (*page)->nxt;
		else
		{
			g_zone[2].total_size = 0; 
			g_zone[2].last = NULL;
			g_zone[2].page = NULL;
		}
		munmap((*page), (*page)->size + sizeof(t_page));
	}
	else if ((p->size == (*page)->size - sizeof(t_block)) && (*page) != g_zone[i].page)
	{
		if (i == 2 && (*page) == g_zone[2].last && (*prev))
			g_zone[2].last = (*prev);
		(*prev) ? (*prev)->nxt = NULL : 0;
		((*prev) && (*page)->nxt) ? (*prev)->nxt = (*page)->nxt : 0;
		munmap((*page), (*page)->size + sizeof(t_page));
	}
}

void		large_free(void *ptr)
{
	t_block		*p;
	t_page		*page;
	t_page		*prev;

	page = g_zone[2].page;
	p = (t_block *)(page + 1);
	while (page)
	{
		if (((t_block *)(page + 1) + 1) == ptr)
		{
			p = (t_block *)(page + 1);
			delete_page(p, &page, &prev, 2);
			return ;
		}
		prev = page;
		page = page->nxt;
	}
}

void		tiny_small_free(void *ptr)
{
	int			i;
	t_block		*p;
	t_page		*page;
	t_page		*prev;

	i = 0;
	p = NULL;
	page = NULL;
	prev = NULL;
	while (i <= 1)
	{
		page = g_zone[i].page;
		while (page)
		{
			if ((char*)ptr >= (char*)page &&
					(char*)ptr <= (char*)(page) + (i == 0) ? N : M)
			{
				p = (t_block *)(page + 1);
				defrag(&p, ptr);
			}
			delete_page(p, &page, &prev, i);
			prev = page;
			page = page->nxt;
		}
		i++;
	}
}

void		ft_free(void *ptr)
{
	tiny_small_free(ptr);
	large_free(ptr);
}